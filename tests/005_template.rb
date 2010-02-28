require 'test/unit'
require 'rbclips'

class Test_Template < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Template)
    assert Clips::Template.constants.member?(:Creator)
  end

  def test_new_hash
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => [:name, 'age'] }
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => %w(name age) }
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => {:name => {:multislot => false}, 'age' => {:default => 30}} }
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => {:name => {:multislot => false}, 'age' => {'default' => 30}} }
    assert_raise(Clips::UsageError)     { Clips::Template.new :name => 'human', :slots => %w() }
    assert_raise(Clips::UsageError)     { Clips::Template.new :name => 'human', :slots => {} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :type => [:any] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 3 }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human' }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :slots => [:name, :age] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => [2, 1, 4] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { 2 => {:default => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {:pico => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {'pico' => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {2 => 30}} }

    a = Clips::Template.new :name => 'human', :slots => [:name, 'age']
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {}, :age => {} }

    a = Clips::Template.new :name => 'human', :slots => %w(name age)
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {}, :age => {} }

    a = Clips::Template.new :name => 'human', :slots => {:name => { :multislot => false}, :age => { :default => 30}}
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }

    a = Clips::Template.new :name => 'human', :slots => {:name => { :multislot => false}, :age => { 'default' => 30}}
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Constraint::Creator.new }
    assert_nothing_raised               { Clips::Template.new('human') {|s| s.slot 'name' } }
    assert_nothing_raised               { Clips::Template.new('human') {|s| s.slot 'name', {} } }
    assert_nothing_raised               { Clips::Template.new('human') {|s| s.slot :name, :multislot => false; s.slot 'age', :default => 3 } }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new(3) {|s| s.slot 'name' } }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new('human') {|s| s.slot :name, :pico => false; s.slot 'age', :default => 3 } }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new('human') {|s| s.slot :name, 'pico' => false; s.slot 'age', :default => 3 } }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new('human') {|s| s.slot :name, 10 => false; s.slot 'age', :default => 3 } }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new('human') {|s| s.slot 7, :multislot => false; s.slot 'age', :default => 3 } }

    a = Clips::Template.new 'human' do |s| 
      s.slot :name
      s.slot 'age'
    end
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {}, :age => {} }

    a = Clips::Template.new 'human' do |s|
      s.slot :name, :multislot => false
      s.slot :age, :default => 30
    end
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }

    a = Clips::Template.new 'human' do |s|
      s.slot :name, :multislot => false
      s.slot 'age', 'default' => 30
    end
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }
  end

  def test_new_string
    a = Clips::Template.new 'human', [:name, 'age']
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {}, :age => {} }

    a = Clips::Template.new 'human', %w(name age)
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {}, :age => {} }

    a = Clips::Template.new 'human', {:name => { :multislot => false}, :age => { :default => 30}}
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }

    a = Clips::Template.new 'human', {:name => { :multislot => false}, :age => { 'default' => 30}}
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }
  end

  def test_new_redefinitions
    assert_raise(Clips::ArgumentError) { Clips::Template.new :name => 'human', :slots => %w(ahoj ahoj) }
    assert_raise(Clips::ArgumentError) { Clips::Template.new('human') {|t| t.slot :ahoj; t.slot :ahoj} }
    # This will pass - very very strange
    #assert_raise(Clips::ArgumentError) { Clips::Template.new :name => 'human', :slots => { :ahoj => {}, :ahoj => {} } }  
  end

  def test_new_template_name_validation
    assert_raise(Clips::ArgumentError) { Clips::Template.new :name => 'hum  an', :slots => %w(ahoj cu) }
    assert_raise(Clips::ArgumentError) { Clips::Template.new('hu man') {|t| t.slot :ahoj; t.slot :abcd} }


    assert_raise(Clips::ArgumentError) { Clips::Template.new :name => 'human', :slots => ['a hoj'] }
    assert_raise(Clips::ArgumentError) { Clips::Template.new :name => 'human', :slots => {'ah  oj' => {}} }
    assert_raise(Clips::ArgumentError) { Clips::Template.new('human') {|t| t.slot :ahoj; t.slot 'a b c d e f'} }
  end

  def test_constraint
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => { :age => {:constraint => {:type => :integer}}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :age => {:constraint => {:xxxx => :integer}}} }

    a = Clips::Template.new :name => 'human', :slots => { :age => {:constraint => {:type => :integer}}}
    assert_equal Clips::Constraint, a.instance_eval { @slots[:age][:constraint].class }
    assert_equal "(type INTEGER ) ", a.instance_eval { @slots[:age][:constraint].to_s } 
  end

  def test_to_s
    c = Clips::Template.new :name => 'human', :slots => { :a => { :multislot => true } }
    assert_equal c.to_s, "(deftemplate human (multislot a) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => 30 } }
    assert_equal c.to_s, "(deftemplate human (slot a (default 30)) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => 'ahoj' } }
    assert_equal c.to_s, "(deftemplate human (slot a (default \"ahoj\")) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => "ahoj" } }
    assert_equal c.to_s, "(deftemplate human (slot a (default \"ahoj\")) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default =>  :ahoj } }
    assert_equal c.to_s, "(deftemplate human (slot a (default ahoj)) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => :none } }
    assert_equal c.to_s, "(deftemplate human (slot a (default ?NONE)) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => :derive } }
    assert_equal c.to_s, "(deftemplate human (slot a (default ?DERIVE)) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => "ahoj" } }
    assert_equal c.to_s, "(deftemplate human (slot a (default \"ahoj\")) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :default => 30, :default_dynamic => true } }
    assert_equal c.to_s, "(deftemplate human (slot a (default-dynamic 30)) )"

    c = Clips::Template.new :name => 'human', :slots => { :a => { :constraint => { :type => :integer, :cardinality => 2..23} } }
    assert_equal c.to_s, "(deftemplate human (slot a (type INTEGER ) (cardinality 2 23) ) )"
  end

  def test_equal
    t = Proc.new do |hash| 
      a = Clips::Template.new hash
      b = Clips::Template.new hash
      assert a == b
      assert a.eql? b
      assert a.equal? b

      c = a.dup
      assert c == a
      assert c == b
    end

    t.call :name => 'human', :slots => { :a => { :multislot => true } }
    t.call :name => 'human', :slots => { :a => { :default => 30 } }
    t.call :name => 'human', :slots => { :a => { :default => 'ahoj' } }
    t.call :name => 'human', :slots => { :a => { :default => "ahoj" } }
    t.call :name => 'human', :slots => { :a => { :default =>  :ahoj } }
    t.call :name => 'human', :slots => { :a => { :default => :none } }
    t.call :name => 'human', :slots => { :a => { :default => :derive } }
    t.call :name => 'human', :slots => { :a => { :default => "ahoj" } }
    t.call :name => 'human', :slots => { :a => { :default => 30, :default_dynamic => true } }
    t.call :name => 'human', :slots => { :a => { :constraint => { :type => :integer, :cardinality => 2..23} } }
  end
  
  def test_destroy
    t = Clips::Template.new :name => 'animal', :slots => %w(name)
    t.save

    f = Clips::Fact.new t, :name => "Azor"
    f.save

    assert_raise(Clips::InUseError)  { t.destroy! }
    assert f.destroy!
    assert t.destroy!
  end

  def test_saving
    t = Proc.new do |hash|
      c = Clips::Template.new hash
      assert c.save
      a = c.dup

      assert a.destroy!
      assert !c.destroy!
    end

    t.call :name => 'humana', :slots => { :a => { :multislot => true } }
    t.call :name => 'humana', :slots => { :a => { :default => 30 } }
    t.call :name => 'humana', :slots => { :a => { :default => 'ahoj' } }
    t.call :name => 'humana', :slots => { :a => { :default => "ahoj" } }
    t.call :name => 'humana', :slots => { :a => { :default =>  :ahoj } }
    t.call :name => 'humana', :slots => { :a => { :default => :none } }
    t.call :name => 'humana', :slots => { :a => { :default => :derive } }
    t.call :name => 'humana', :slots => { :a => { :default => "ahoj" } }
    t.call :name => 'humana', :slots => { :a => { :default => 30, :default_dynamic => true } }
    t.call :name => 'humana', :slots => { :a => { :constraint => { :type => :integer, :cardinality => 2..23}, :multislot => true } }
  end

  def test_saved
    a = Clips::Template.new :name => 'animal', :slots => %w(race age)
    assert a.save
    assert a.saved?
    assert a.destroy!
    assert !a.saved?
  end

  def test_load
    a = Clips::Template.new :name => 'animal', :slots => %w(race age)
    assert a.save
    
    b = Clips::Template.load 'animal'
    assert_equal a, b
    c = Clips::Template.load :animal
    assert_equal a, c

    assert_nil Clips::Template.load 'non-existent'

    assert c.destroy!
    assert !a.destroy!
    assert !b.destroy!
  end

  def test_all
    a = Clips::Template.new :name => 'animal', :slots => %w(race age)
    assert a.save
    b = Clips::Template.new :name => 'mammal', :slots => %w(race age)
    assert b.save

    assert_equal Clips::Template.all, [a, b]

    assert a.destroy!
    assert b.destroy!
  end
end
