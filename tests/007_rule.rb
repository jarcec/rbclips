require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase
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
end
