require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Rule)
    assert Clips::Rule.constants.member?(:Creator)
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Rule::Creator.new }
    assert_raise(Clips::ArgumentError)  { Clips::Rule.new(3){}  }

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animal ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animal ?b))"
    end  
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) (child-of ?a ?b) => (assert (animal ?b)))"
  end

  def test_creator_patter
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :a
      r.rhs '(assert (animal ?a))'
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) => (assert (animal ?a)))"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :one
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?) => )"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :all
    end
    assert_equal rule.to_s, "(defrule animal-mammal (animal $?) => )"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', 1, "ahoj", 2.3
    end
    assert_equal rule.to_s, '(defrule animal-mammal (animal 1 "ahoj" 2.3) => )'

    assert_raise(Clips::UsageError)  do
      Clips::Rule.new 'x' do |r|
      end
    end

    t = Clips::Template.new :name => 'human', :slots => %w(age name)
    rule = Clips::Rule.new 'humanize' do |r|
      r.pattern t, :age => :a, :name => 'jarcec'
    end
    assert_equal rule.to_s, '(defrule humanize (human (age ?a) (name "jarcec")) => )'

    assert_raise(Clips::ArgumentError)  do
      Clips::Rule.new 'human' do |r|
        r.pattern t, :nick => 'jarcec'
      end
    end
  end

  def test_creator_retract
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.retract 'animal', :a
    end
    assert_equal rule.to_s, '(defrule animal-mammal ?rbclips-0 <- (animal ?a) => (retract ?rbclips-0))'

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.retract 'animal', :a
      r.retract 'mammal', :a
      r.retract 'child-of', :a
    end
    assert_equal rule.to_s, '(defrule animal-mammal ?rbclips-0 <- (animal ?a) ?rbclips-1 <- (mammal ?a) ?rbclips-2 <- (child-of ?a) => (retract ?rbclips-0) (retract ?rbclips-1) (retract ?rbclips-2))'

    t = Clips::Template.new :name => 'human', :slots => %w(age name)
    rule = Clips::Rule.new 'humanize' do |r|
      r.retract t, :age => :a, :name => 'jarcec'
    end
    assert_equal rule.to_s, '(defrule humanize ?rbclips-0 <- (human (age ?a) (name "jarcec")) => (retract ?rbclips-0))'
  end

  def test_creator_or_and_not
    rule = Clips::Rule.new 'x' do |r|
      r.retract 'a', :a
      r.or do |o|
        o.retract 'b', :a
        o.retract 'c', :a
      end
    end
    assert_equal rule.to_s, '(defrule x ?rbclips-0 <- (a ?a) (or ?rbclips-1 <- (b ?a) ?rbclips-2 <- (c ?a)) => (retract ?rbclips-0) (retract ?rbclips-1) (retract ?rbclips-2))'

    rule = Clips::Rule.new 'x' do |r|
      r.retract 'a', :a
      r.or do |o|
        o.retract 'b', :a
        o.and do |a|
          a.retract 'c', :a
          a.retract 'd', :a
        end
      end
      r.retract 'd', :a
      r.not {|n| n.retract 'e', :a }
    end

    assert_raise(Clips::UsageError)  do
      Clips::Rule.new 'x' do |r|
        r.or {|o| }
      end
    end
  end

  def test_save_destroy!
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert rule.save
    assert rule.destroy!
    assert !rule.destroy!
  end

  def test_load
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert rule.save
    copy = Clips::Rule.load 'animal-mammal'
    assert copy.destroy!
    assert !rule.destroy!
  end

  def test_equal
    a = Clips::Rule.new 'animal-mammal-da' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert_raise(Clips::UsageError) { assert_equal a, a }
    assert a.save

    b = Clips::Rule.load 'animal-mammal-da'
    assert_equal a, b

    assert_not_equal a, 3
    assert_not_equal b, "ahoj"

    assert a.destroy!
  end

  def test_all
    a = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    b = Clips::Rule.new 'animal-mammal2' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert a.save
    assert b.save

    assert_equal [a, b], Clips::Rule.all
  end

  # Dump class for testing purposes
  class Dump
    def initialize
      @num = 0
    end
    def add(num)
      @num += num
    end
  end

  def test_rcall
    dump = Dump.new

    rule = Clips::Rule.new "counter" do |r|
      r.pattern 'add', :number
      r.rcall dump, :add, :number
    end
    rule.save
    Clips::Fact.new("add", [3]).save
    Clips::Fact.new("add", [5]).save
    Clips::Base.run

    assert_equal dump.instance_eval{@num}, 8
  end
end
